'use strict';

// Compare the packed emitter with an independent x86 byte encoder. In particular,
// a correct arithmetic result alone would not detect a remapped instruction form.
function fixture(header) {
    function macro(name) {
        const lines = header.split(/\r?\n/);
        const start = lines.findIndex(line => line.startsWith('#define ' + name + '('));
        if (start < 0) throw new Error('Missing ' + name);
        let end = start;
        while (lines[end].endsWith('\\')) ++end;
        return lines.slice(start, end + 1).join('\n') + '\n';
    }
    let code = '#include <stdio.h>\n' + ['OBFH_C_SLOT', 'OBFH_C_ENCODING', 'OBFH_C_ARG'].map(macro).join('');
    code += '#if defined(__x86_64__)\n#define ADDRESS "leaq 1f(%%rip),%0; jmp 2f;1:"\n#else\n#define ADDRESS "leal 1f,%0; jmp 2f;1:"\n#endif\n';
    const cases = [];
    for (const kind of [0, 3]) for (let form = 0; form < 3; ++form)
        for (const key of [0, 1, 0x80000000, 0xffffffff]) cases.push({ kind, form, key });
    for (let rotation = 0; rotation < 4; ++rotation)
        for (let count = 1; count <= (rotation < 2 ? 31 : 1); ++count) cases.push({ kind: 1, rotation, count });
    for (let scale = 0; scale < 3; ++scale) for (let complement = 0; complement < 2; ++complement)
        cases.push({ kind: 2, scale, complement });
    const expected = [];
    cases.forEach(({ kind, form = 0, key = 0, rotation = 0, count = 1, scale = 0, complement = 0 }, id) => {
        let bytes;
        if (kind === 0 || kind === 3) bytes = [[0x35, 0x05, 0x2d][form], ...Array.from({ length: 4 }, (_, n) => (key >>> (8 * n)) & 255)];
        else if (kind === 1) bytes = rotation < 2 ? [0xc1, 0xc0 + 8 * rotation, count] : [0xf7, rotation === 2 ? 0xd0 : 0xd8];
        else bytes = complement ? [0xf7, 0xd0] : [0x8d, 0x04, 0x40 + 0x40 * scale];
        expected.push([...bytes, 0xcc]);
        code += `static const unsigned char *probe_${id}(void){
enum {__c_kind001=${kind},__c_form001=${form},__c_key001=${key}u,__c_rform001=${rotation},__obfh_r00=${count},__c_lform001=${complement},__c_scale00=${scale}};
OBFH_C_ENCODING(0,0,1);
const unsigned char *result;
__asm__ __volatile__(ADDRESS OBFH_C_SLOT(1) ".byte 0xcc;2:" : "=r"(result) : OBFH_C_ARG(0,0,1));
return result;}
`;
    });
    code += `int main(void){
const unsigned char *actual[]={${cases.map((_, i) => `probe_${i}()`).join(',')}};
static const unsigned char expected[][6]={${expected.map(bytes => '{' + bytes.join(',') + '}').join(',')}};
static const unsigned char lengths[]={${expected.map(bytes => bytes.length).join(',')}};
for(unsigned i=0;i<sizeof(lengths);++i)for(unsigned b=0;b<lengths[i];++b)
if(actual[i][b]!=expected[i][b]){fprintf(stderr,"packed CFLOW: case %u byte %u\\n",i,b);return 1;}
if(puts("CFLOW_PACKING_PASS")==EOF||fflush(stdout))return 2;return 0;}
`;
    return code;
}
module.exports = { fixture };
