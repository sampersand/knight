import { Identifier } from './Identifier.js';
import { Integer } from './Integer.js';

let ident = new Identifier("foo");

ident.assign(new Integer(123));

console.log(ident.run().add(ident.run()).toString());
