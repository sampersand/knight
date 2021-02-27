import { Value } from './value';

export class Literal<T> extends Value {
	protected data: T;

	constructor(data: T) {
		super();

		this.data = data;
	}

	toString(): string {
		return String(this.data);
	}

	
	run() {
		return this;
	}

	toInt(): number {
		return Number(this.data);
	}

	toBool(): boolean {
		return Boolean(this.data);
	}

	eql(rhs: any): boolean {
		let z: any = rhs;
		return rhs instanceof this.constructor && (this.data === z.data);
	}
}
