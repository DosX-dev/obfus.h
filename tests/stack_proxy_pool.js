const fs = require('fs');
const path = require('path');

// Execute native entries, rather than merely checking that their live guard skips them.
// The reference build keeps bodies inside their original frames and call graphs.
async function runSuite({ arch, compiler, directory, check, compile, execute, assert }) {
    for (const seed of [1, 2]) {
        await check(`${arch}/stack proxies/pooled native entries match reference/seed ${seed}`, async () => {
            const flags = [
                'OBFH_DISABLE_CONTROL_FLOW=1',
                'OBFH_DISABLE_ANTIDEBUG=1',
                `OBFH_SEED=${seed}u`
            ];
            const fixture = path.join(__dirname, 'stack_proxy_pool.c');
            const pooled = await compile(
                compiler, directory, `${arch}-stack-proxy-pool-${seed}.exe`, fixture, flags
            );
            const reference = await compile(
                compiler, directory, `${arch}-stack-proxy-pool-reference-${seed}.exe`,
                fixture, [...flags, 'OBFH_TEST_POOL_REFERENCE=1']
            );
            const actual = await execute(pooled, 'STACK_PROXY_POOL_PASS');
            const expected = await execute(reference, 'STACK_PROXY_POOL_PASS');
            assert(actual.stdout === expected.stdout, 'pooled native bodies differ from their reference');
            const binary = fs.readFileSync(pooled);
            for (const alias of ['.Lobfh_sf_', '.Lsp', '.Lsl', '.Lsa']) {
                assert(!binary.includes(Buffer.from(alias)), `assembler alias leaked into the PE: ${alias}`);
            }
        });
    }
}

module.exports = { runSuite };
