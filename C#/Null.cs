using System;
using Knight.Ops;

namespace Knight
{
	public class Null : IValue, IEquatable<IValue>
	{
		public static IValue Parse(ref string stream) {
			if (stream[0] != 'N') {
				return null;
			}

			do {
				stream = stream.Substring(1);
			} while(char.IsUpper(stream[0]));

			return new Null();
		}

		public IValue Run() => this;
		public void Dump() => Console.Write("Null()");
		public override string ToString() => "null";
		public bool ToBoolean() => false;
		public long ToNumber() => 0;

		public bool Equals(IValue obj) => GetType() == obj.GetType();

	}
}