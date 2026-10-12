import { readFileSync } from 'node:fs';
import { LEVEL_LABELS } from '../core/config.js';
import { emitFamily } from './family-emitter.js';
import { emitNetwork } from './bit-network.js';
import { emitTriangular } from './triangular.js';
export function emit({ ir, target }, { mutate = null } = {}) {
    const hardcore = ['hardcore', 'extreme'].includes(ir.level),
        mask = 2 ** ir.bits - 1;
    const stages = mutate === 'drop' ? ir.stages.slice(1) : ir.stages;
    const rows = [],
        calls = [];
    const slot = (i) => ir.storage[i];
    function expression(node, coordinates = null) {
        const source = index => `s[${slot(coordinates ? coordinates[index] : index)}]`;
        if (node.kind === 'source') return source(node.index);
        if (node.kind === 'ref') return `q${node.index}`;
        if (node.kind === 'select') return `(p_equ(p_and(${source(node.selector)},${node.value}u),0u)?${expression(node.right, coordinates)}:${expression(node.left, coordinates)})`;
        const x = expression(node.left, coordinates);
        if (node.kind === 'add') return `bounded(p_add(${x},${expression(node.right, coordinates)}))`;
        if (node.kind === 'xor') return `p_xor(${x},${expression(node.right, coordinates)})`;
        if (node.kind === 'and') return `p_and(${x},${expression(node.right, coordinates)})`;
        if (node.kind === 'or') return `p_or(${x},${expression(node.right, coordinates)})`;
        if (node.kind === 'sub') return `bounded(p_sub(${x},${expression(node.right, coordinates)}))`;
        if (node.kind === 'rot') return `rot(${x},${node.value}u)`;
        if (['shlXor', 'shrXor'].includes(node.kind)) return `fold_${node.kind === 'shlXor' ? 'left' : 'right'}(${x},${node.value}u)`;
        if (node.kind === 'mul') return `bounded(p_mul(${x},${node.value}u))`;
        if (node.kind === 'constant') return `bounded(p_add(${x},${node.value}u))`;
        if (node.kind === 'xorConstant') return `p_xor(${x},${node.value}u)`;
        throw new Error('Unknown emitted expression');
    }
    function update(destination, current, term, form) {
        if (form < 3) return `${destination}=bounded(p_${['xor', 'add', 'sub'][form]}(${current},${term}));`;
        // Capture each dependent expression once, even when a rotate uses its count twice.
        if (form === 3) return `{unsigned factor=p_or(p_shl(p_and(${term},7u),1u),1u);${destination}=bounded(p_mul(${current},factor));}`;
        if (form === 4) return `{unsigned count=p_add(p_and(${term},7u),1u);${destination}=rot(${current},count);}`;
        throw new Error('Unknown emitted update');
    }
    for (const s of stages) {
        if (s.kind === 'triangular') {
            calls.push(emitTriangular(s, ir.bits, slot));
            continue;
        }
        if (s.kind === 'bitnetwork') {
            calls.push(emitNetwork(s, ir.bits, slot));
            continue;
        }
        if (['substitute', 'matrix', 'spn', 'controlled', 'bitshuffle', 'weave'].includes(s.kind)) {
            calls.push(emitFamily(s, ir.bits, slot));
            continue;
        }
        if (s.kind === 'affine' && s.ops[0].sequence) {
            calls.push(
                s.ops
                    .map((c) => {
                        const v = `s[${slot(c.i)}]`;
                        return c.sequence
                            .map((op) => {
                                if (op.kind === 'rot') return `${v}=rot(${v},${op.value}u);`;
                                if (op.kind === 'shlXor' || op.kind === 'shrXor') return `${v}=bounded(p_xor(${v},p_${op.kind === 'shlXor' ? 'shl' : 'shr'}(${v},${op.value}u)));`;
                                return `${v}=bounded(p_${op.kind}(${v},${op.value}u));`;
                            })
                            .join('\n');
                    })
                    .join('\n') + '\nobserve(s);'
            );
            continue;
        }
        if (s.kind === 'couple' && s.ops[0].expr) {
            calls.push(
                s.ops
                    .map((c) => {
                        const v = `s[${slot(c.i)}]`,
                            term = expression(c.expr);
                        return update(v, v, term, c.form);
                    })
                    .join('\n') + '\nobserve(s);'
            );
            continue;
        }
        if (['feistel', 'braid'].includes(s.kind) && s.ops[0].expr) {
            calls.push(
                `{unsigned t[WIDTH];\n${(s.nodes || []).map((node, i) => `unsigned q${i}=${expression(node, s.sources)};`).join('\n')}\n${Array.from({ length: ir.width }, (_, i) => `t[${i}]=s[${i}];`).join('\n')}\n${s.ops.map((c, i) => update(`t[${slot(s.targets[i])}]`, `s[${slot(s.targets[i])}]`, expression(c.expr, s.sources), c.form)).join('\n')}\n${s.order.map((source, i) => `s[${slot(i)}]=t[${slot(source)}];`).join('\n')}\n} observe(s);`
            );
            continue;
        }
        if (s.kind !== 'permute') throw new Error('Unknown emitted stage');
        const start = rows.length;
        rows.push(...s.order.map((source, i) => `{${slot(i)},${slot(source)}}`));
        calls.push(`step_permute(s, ops + ${start}, ${rows.length - start}); observe(s);`);
    }
    const maths = ['ADD', 'SUB', 'MUL', 'XOR', 'OR', 'AND', 'SHL', 'SHR'];
    const native = { ADD: '+', SUB: '-', MUL: '*', XOR: '^', OR: '|', AND: '&', SHL: '<<', SHR: '>>' };
    const opFunctions = maths
        .map(
            (op) =>
                `static unsigned p_${op.toLowerCase()}(unsigned a,unsigned b){ return ${hardcore ? `VM_${op}(a,b)` : `(a ${native[op]} b)`}; }`
        )
        .join('\n');
    const groups = [];
    for (let i = 0; i < ir.groups.length; ++i) {
        const [begin, end] = ir.groups[i];
        groups.push(
            `static void stage_${i}(unsigned *s){ ${ir.level === 'easy' ? '' : i % 2 ? 'BREAK_STACK_CFLOW;' : 'STACK_PROXY_FUNCTIONS;'}\n${calls.slice(begin, end).join('\n')} }`
        );
    }
    const source = `// Generated challenge. Owner answers are stored separately, never in this translation unit.
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ole2.h>
#include <shellapi.h>
#define OBFH_BUILD_SEED ${ir.seed}u
${ir.level !== 'easy' ? '#define CFLOW_V2 1\n#define ANTIDEBUG_V2 1' : ''}
${hardcore ? '#define VIRT 1' : ''}
#ifdef SELFTEST
static unsigned long vm_steps;
void daily_vm_step(unsigned op, unsigned pc) { (void)op; (void)pc; ++vm_steps; }
#endif
#include "obfus.h"
#define WIDTH ${ir.width}
#define BITS ${ir.bits}
#define MASK ${mask}u
#define PASSWORD_LENGTH ${ir.length}
#define DAILY_DIFFICULTY_COLOR ${{ easy: 'RGB(99,211,159)', medium: 'RGB(239,184,88)', hardcore: 'RGB(244,105,113)', extreme: 'RGB(232,178,255)' }[ir.level]}
typedef struct { unsigned i,j; } Operation;
static const Operation ops[] = {${rows.length ? rows.join(',\n') : '{0}'}};
static const unsigned storage[] = {${ir.storage.join(',')}};
static const unsigned expected[] = {${target.map((n, i) => n ^ ((i * 7919 + ir.seed) & mask)).join(',')}};
${opFunctions}
static unsigned p_equ(unsigned a,unsigned b){return ${hardcore ? 'VM_EQU(a,b)' : '(a==b)'};}
static unsigned bounded(unsigned x){return p_and(x,MASK);}
static unsigned rot(unsigned x,unsigned n){return bounded(p_or(p_shl(x,n),p_shr(x,BITS-n)));}
static unsigned fold_left(unsigned x,unsigned n){return bounded(p_xor(x,p_shl(x,n)));}
static unsigned fold_right(unsigned x,unsigned n){return p_xor(x,p_shr(x,n));}
#ifdef SELFTEST
static int tracing;
static void observe(unsigned *s){if(tracing){unsigned i;for(i=0;i<WIDTH;i++)printf("%u%c",s[storage[i]],i+1==WIDTH?'\\n':' ');}}
#else
#define observe(s) ((void)0)
#endif
static void step_permute(unsigned *s,const Operation *p,unsigned count){
 unsigned t[WIDTH],n;for(n=0;n<count;n++)t[p[n].i]=s[p[n].j];for(n=0;n<WIDTH;n++)s[n]=t[n];
}
${groups.join('\n')}
static int verify_state(unsigned *s){
 unsigned i,bad=0;
 ${ir.groups.map((_, i) => `stage_${i}(s);`).join('\n')}
 for(i=${mutate === 'coordinate' ? 1 : 0};i<WIDTH;i++)bad=p_or(bad,p_xor(s[storage[i]],p_xor(expected[i],(i*7919u+${ir.seed}u)&MASK)));
 ${hardcore ? 'VM_IF(VM_EQU(bad,0))' : 'if(bad==0)'}{int accepted=1;RET_BY_VAR(accepted);}return 0;
}
static int verify_bytes(const unsigned char *password,size_t length){
 unsigned s[WIDTH],i;
 if(length!=PASSWORD_LENGTH)return 0;
 for(i=0;i<PASSWORD_LENGTH;i++)if(password[i]<33||password[i]>126)return 0;
 for(i=0;i<WIDTH;i++)s[storage[i]]=${ir.bits === 8 ? 'password[i]' : 'password[2*i]|((unsigned)password[2*i+1]<<8)'};
 return verify_state(s);
}
static int verify(const char *password){return verify_bytes((const unsigned char *)password,strlen(password));}
#ifdef SELFTEST
static int hex_digit(unsigned char c){if(c>='0'&&c<='9')return c-'0';if(c>='a'&&c<='f')return c-'a'+10;return -1;}
int main(void){
 char line[1024];ANTI_DEBUG;
 while(fgets(line,sizeof(line),stdin)){
 size_t n=strlen(line);int accepted;unsigned long before=vm_steps;
 while(n&&(line[n-1]=='\\r'||line[n-1]=='\\n'))line[--n]=0;
 tracing=line[0]=='T'&&line[1]==' ';accepted=0;
 if((line[0]=='H'||line[0]=='B')&&line[1]==' '){
  unsigned char bytes[512];size_t k,count=(n-2)/2;int valid=(n-2)%2==0;
  for(k=0;k<count;k++){int a=hex_digit(line[2+2*k]),b=hex_digit(line[3+2*k]);if(a<0||b<0){valid=0;bytes[k]=0;}else bytes[k]=(a<<4)|b;}
  if(valid&&line[0]=='H')accepted=verify_bytes(bytes,count);
  else if(valid&&count==PASSWORD_LENGTH){unsigned s[WIDTH];tracing=1;for(k=0;k<WIDTH;k++)s[storage[k]]=${ir.bits === 8 ? 'bytes[k]' : 'bytes[2*k]|((unsigned)bytes[2*k+1]<<8)'};accepted=verify_state(s);}
 }else accepted=verify(line+(tracing?2:0));
 printf("RESULT %d %lu\\n",accepted,vm_steps-before);
 }return 0;
}
#else
${readFileSync(new URL('../ui/gui.c', import.meta.url), 'utf8')
            .replace('/* @GUI_STYLE@ */', readFileSync(new URL('../ui/gui-style.c', import.meta.url), 'utf8'))
            .replaceAll('@DATE@', ir.date)
            .replaceAll('@ROUND@', String(ir.round))
            .replaceAll('@LEVEL@', ir.level === 'extreme' ? LEVEL_LABELS[ir.level] : LEVEL_LABELS[ir.level][0] + LEVEL_LABELS[ir.level].slice(1).toLowerCase())
            .replaceAll('@COLOR@', { easy: 'RGB(99,211,159)', medium: 'RGB(239,184,88)', hardcore: 'RGB(244,105,113)', extreme: 'RGB(232,178,255)' }[ir.level])}
#endif
`;
    return mutate === 'vm' ? source.replace('return VM_ADD(a,b);', 'return 0;') : source;
}
