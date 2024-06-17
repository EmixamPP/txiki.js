const core = globalThis[Symbol.for('tjs.internal.core')];

import { URLPattern } from 'urlpattern-polyfill';
import { URL, URLSearchParams } from 'whatwg-url';

globalThis.URL = URL;
globalThis.URLPattern = URLPattern;
globalThis.URLSearchParams = URLSearchParams;

// TODO: straggly, without the wrapped console.log, the runtime will abort
// so that we cannot write:
// globalThis.URL.createObjectURL = core.createObjectURL;
// globalThis.URL.revokeObjectURL = core.revokeObjectURL;
// more investigation is needed
globalThis.URL.createObjectURL = (object) => {
    console.log('createObjectURL', object);
    return core.createObjectURL(object);
}
globalThis.URL.revokeObjectURL = (url) => {
    console.log('revokeObjectURL', url);
    return core.revokeObjectURL(url);
}
