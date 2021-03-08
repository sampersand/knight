using System;
using Knight.Ops;

namespace Knight
{
	public class Boolean : Literal<bool>, IComparable<IValue>
	{
		public Boolean(bool data) : base(data) {}

		public IValue Parse(ref string stream) {
			if (stream[0] != 'T' && stream[0] != 'F') {
				return null;
			}

			var isTrue = stream[0] == 'T';

			do {
				stream = stream.Substring(1);
			} while(char.IsUpper(stream[0]));

			return new Boolean(isTrue);<
		}

		public override bool ToBoolean() => _data;
		public override long ToNumber() => _data ? 1 : 0;

		public int CompareTo(IValue obj) => _data.CompareTo(obj.ToBoolean());
	}
}