using System;
using Knight.Ops;
namespace Knight
{
	public class Number : Literal<long>, IAdd, ISub, IMul, IDiv, IMod, IPow, IComparable<IValue>
	{
		public Number(long data) : base(data) {}

		public static IValue Parse(ref string stream) {
			if (!char.IsDigit(stream[0]))
				return null;

			long value = 0;
	
			do {
				value = value * 10 + (stream[0] - '0');

				stream = stream.Substring(1);
			} while(stream != "" && Char.IsDigit(stream[0])) ;

			return new Number(value);
		}

		public override void Dump() => Console.Write($"Number({this})");

		public override bool ToBoolean() => _data != 0;
		public override long ToNumber() => _data;

		public int CompareTo(IValue other) => _data.CompareTo(other.ToNumber());		

		public IValue Add(IValue rhs) => new Number(_data + rhs.ToNumber());
		public IValue Sub(IValue rhs) => new Number(_data - rhs.ToNumber());
		public IValue Mul(IValue rhs) => new Number(_data * rhs.ToNumber());
		public IValue Div(IValue rhs) {
			var rlong = rhs.ToNumber();
			
			if (rlong == 0) {
				throw new RuntimeException("Cannot divide by zero!");
			}

			return new Number(_data / rlong);
		}
		
		public IValue Mod(IValue rhs) {
			var rlong = rhs.ToNumber();
			
			if (rlong == 0) {
				throw new RuntimeException("Cannot modulo by zero!");
			}

			return new Number(_data % rlong);
		}
		
		public IValue Pow(IValue rhs) => new Number((long) Math.Pow(_data, rhs.ToNumber()));
	}
}