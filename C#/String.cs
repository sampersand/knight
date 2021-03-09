using System;
using Knight.Ops;

namespace Knight
{
	public class String : Literal<string>, IAdd, IMul, IComparable<IValue>
	{
		public String(string data) : base(data) {}

		public static IValue Parse(ref string stream) {
			var quote = stream[0];
			
			if (quote != '\'' && quote != '\"') {
				return null;
			}

			var start = stream;
			var string_ = "";

			do {
				string_ += stream[0];
				stream = stream.Substring(1);
				if (stream.Length == 0) {
					throw new RuntimeException($"unterminated string, starting at {start}");
				}
			} while (start.Substring(1)[0] != quote);

			return new String(string_);
		}

		public override void Dump() => Console.Write($"String({_data})");

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