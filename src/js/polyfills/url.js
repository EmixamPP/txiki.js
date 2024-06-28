import { URLPattern } from 'urlpattern-polyfill';
import { URL, URLSearchParams } from 'whatwg-url';

globalThis.URL = URL;
globalThis.URLPattern = URLPattern;
globalThis.URLSearchParams = URLSearchParams;

const _objectURLs = new Map();

// TODO: straggly, without the wrapped console.log, the runtime will abort
// more investigation is needed

globalThis.URL.createObjectURL = object => {
    console.log('');

    if  (!(object instanceof Blob)) {
        throw new TypeError('URL.createObjectURL: Argument 1 is not valid for any of the 1-argument overloads.');
    }

    const url = `blob:${crypto.randomUUID()}`;

    _objectURLs.set(url, object);

    return url;
};

globalThis.URL.revokeObjectURL = url => {
    console.log('');

    return _objectURLs.delete(url);
};

globalThis.URL.getObjectURL = url => {
    console.log('');

    return _objectURLs.get(url);
};
