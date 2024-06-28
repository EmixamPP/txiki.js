const core = globalThis[Symbol.for('tjs.internal.core')];
const _Worker = core.Worker;

import { defineEventAttribute } from './event-target';

const kWorker = Symbol('kWorker');

class Worker extends EventTarget {
    constructor(path) {
        super();

        const resolve_worker = (resolve, path, blob) => {
            const worker = new _Worker(path, blob);

            worker.onmessage = msg => {
                this.dispatchEvent(new MessageEvent('message', msg));
            };

            worker.onmessageerror = msgerror => {
                this.dispatchEvent(new MessageEvent('messageerror', msgerror));
            };

            worker.onerror = error => {
                this.dispatchEvent(new ErrorEvent(error));
            };

            resolve(worker);
        };

        this[kWorker] = new Promise((resolve, _) => {
            const maybeObjectURL = URL.getObjectURL(path);

            if (maybeObjectURL) {
                maybeObjectURL.text().then(blob => resolve_worker(resolve, path, blob));
            } else {
                resolve_worker(resolve, path);
            }
        });
    }

    postMessage(message) {
        this[kWorker].then(worker => worker.postMessage(message));
    }

    terminate() {
        this[kWorker].then(worker => worker.terminate());
    }
}

const workerProto = Worker.prototype;

defineEventAttribute(workerProto, 'message');
defineEventAttribute(workerProto, 'messageerror');
defineEventAttribute(workerProto, 'error');

Object.defineProperty(window, 'Worker', {
    enumerable: true,
    configurable: true,
    writable: true,
    value: Worker
});
