const core = globalThis[Symbol.for('tjs.internal.core')];

import { URLPattern } from 'urlpattern-polyfill';
import { URL, URLSearchParams } from 'whatwg-url';

globalThis.URL = URL;
globalThis.URLPattern = URLPattern;
globalThis.URLSearchParams = URLSearchParams;

// TODO: straggly, without the wrapped console.log, the runtime will abort
// more investigation is needed
globalThis.URL.createObjectURL = (object) => {
    console.log('');
    const url = `blob:${crypto.randomUUID()}`;
    return core.createObjectURL(object, url);
}
globalThis.URL.revokeObjectURL = (url) => {
    console.log('');
    return core.revokeObjectURL(url);
}
