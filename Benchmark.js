const TARGET_URL = '';
const C = 0;
const N = 0;

eval(`
(async () => {
    const request = Math.ceil(${N} / ${C});
    let s = process.hrtime();
    await Promise.all([
        ${[...Array(C)].map(_ => `( async () => { for(let i=request;i;i--) await fetch('${TARGET_URL}') })()`).join(',\n        ')}
    ]);
    console.log(\`Execution time \${process.hrtime(s)}ms\`)
})()
`);
