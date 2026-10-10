'use strict';
const fs = require('node:fs'), path = require('node:path');
// Trace only test builds; ordinary builds use the header without instrumentation.
const trace = String.raw`
#ifdef OBFH_TEST_FLOW_TRACE
#undef OBFH_P_STEP
#define OBFH_P_STEP(s,p) ({ \
    unsigned __before=__obfh_flow_state; \
    __obfh_asm__(OBFH_C_TEXT : "+a"(__obfh_flow_state) : OBFH_C_ARGS(s,p) : "cc"); \
    __obfh_flow_tag=OBFH_C_POINT(OBFH_C_NEXT(s)); \
    obfh_test_flow_stage(__obfh_flow_layout,s,p,__before,OBFH_C_POINT(s),__obfh_flow_state,__obfh_flow_tag, \
        __obfh_k##s##p,1,__obfh_a##s##p,__obfh_r##s##p,0,0,0,1,0,1); \
    __builtin_choose_expr((s)==0, ({ obfh_test_flow_visit(); obfh_test_flow_route(__obfh_flow_first); \
        obfh_test_transport_visit(__obfh_flow_layout,__obfh_flow_exit,__obfh_flow_hash&1u); }),((void)0)); \
})
#undef OBFH_P_TERMINAL
#define OBFH_P_TERMINAL(s,p,style) ({ \
    OBFH_P_STEP(s,p); \
    __obfh_asm__("movl $%c[expected], %%edx;" OBFH_P_TERMINAL_TEXT \
        : "=&a"(__obfh_flow_result) : "0"(__obfh_flow_state), \
          [expected] "i"(OBFH_C_POINT(OBFH_C_NEXT(s))),[finish] "i"((style)&7u), \
          [zero_form] "i"((__obfh_flow_hash>>11)%3u) : "edx","ecx","cc"); \
    OBFH_P_TERMINAL_TRACE(style); \
})
#endif
`;
function fixture(header) {
    const start = header.indexOf('// A stage maps a compile-time input representation');
    const end = header.indexOf('#define OBFH_FLOW_CONDITION(', start);
    if (start < 0 || end < start) throw Error('Missing compositional CFLOW core');
    return fs.readFileSync(path.join(__dirname, 'cflow_single_lane.c'), 'utf8')
        .replace('#include "single-lane.h"', header.slice(start, end));
}
module.exports = { trace, fixture };
