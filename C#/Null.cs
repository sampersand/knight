using System;
using Knight.Ops;

namespace Knight
{
	public class Null : IValue, IEquatable<IValue>
	{
		public static Null Parse(Stream stream) {
			if (!stream.StartsWith('N')) {
				return null;
			}

			stream.StripKeyword();

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