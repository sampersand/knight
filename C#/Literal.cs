using System;

namespace Knight
{
	public abstract class Literal<T> : IValue, IEquatable<IValue>
	where T: IEquatable<T>
	{
		protected readonly T _data;

		public Literal(T data) => _data = data;
		public IValue Run() => this;
		public abstract void Dump();

		public override string ToString() => _data.ToString();
		public abstract bool ToBoolean();
		public abstract long ToNumber();

		public bool Equals(IValue obj) => GetType() == obj.GetType() && _data.Equals(((Literal<T>) obj)._data);
	}
}