import createModule from "./wasm/nonogram-solver.js"

createModule().then(wasm_instance => {
    let area;
    let input = wasm_instance._getInputPtr();
    let output = wasm_instance._getOutputPtr();
    const getView = () => {
        return wasm_instance.HEAP16.subarray(output >> 1, (output >> 1) + area);
    }

    wasm_instance._setWrap(wasm_instance.addFunction(() => postMessage([0, getView(), 0, -1, wasm_instance._getTime()]), "v"));
    wasm_instance._setInspect(wasm_instance.addFunction((vec, vi, inference) => postMessage([1, getView(), vec, vi]), "viii"));
    wasm_instance._setFail(wasm_instance.addFunction(() => postMessage([-1]), "v"));

    onmessage = ({data}) => {
        let [inspection, clue, interval] = data;

        area = clue[0] * clue[1];
        wasm_instance.HEAP16.set(clue, input >> 1);
        wasm_instance._solve(inspection, interval);
    };
});
