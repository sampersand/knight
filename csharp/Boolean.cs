using System;

namespace Knight
{
	public class Boolean : Literal<bool>, IComparable<IValue>
	{
		public Boolean(bool data) : base(data) {}

		public static Boolean Parse(Stream stream) {
			if (!stream.StartsWith('T', 'F'))
				return null;

			var ret = new Boolean(stream.Take() == 'T');
			stream.StripKeyword();

			return ret;
		}
		
		public override void Dump() => Console.Write($"Boolean({this})");

		public override string ToString() => _data ? "true" : "false";
		public override bool ToBoolean() => _data;
		public override long ToNumber() => _data ? 1 : 0;

		public int CompareTo(IValue obj) => _data.CompareTo(obj.ToBoolean());
	}
}