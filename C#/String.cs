using System;
using Knight.Ops;

namespace Knight
{
	public class String : Literal<string>, IAdd, IMul, IComparable<IValue>
	{
		public String(string data) : base(data) {}

		public override bool ToBoolean() => _data != "";
		public override long ToNumber() {
			long ret = 0;
			;

			for (var s = _data.TrimStart(); s != "" && char.IsDigit(s[0]); s = s.Substring(1)) {
				ret = ret * 10 + (s[0] - '0');
			}

			return ret;
		}

		public IValue Add( IValue rhs) => new String(_data + rhs);
		public IValue Mul(IValue rhs) {
			var s = "";
			var amnt = rhs.ToNumber();

			for (long i = 0; i < amnt; ++i)
				s += _data;
			return new String(s);
		}

		public int CompareTo(IValue obj) => _data.CompareTo(obj.ToString());
	}
}