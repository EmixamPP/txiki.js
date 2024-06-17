import assert from 'tjs:assert';

const data = JSON.stringify({ foo: 42, bar: 'baz!' });
const worker = `if (self.addSignalListener || self.removeSignalListener) {throw new Error('There are signals in a Worker!');} self.postMessage(${data});`
const blob = new Blob([worker], { type: 'text/javascript' });
const url = URL.createObjectURL(blob);

const w = new Worker(url);
const timer = setTimeout(() => {
    w.terminate();
}, 1000);
w.onmessage = event => {
    const recvData = JSON.stringify(event.data);
    assert.eq(data, recvData, 'Message received matches');
    w.terminate();
    clearTimeout(timer);
};
