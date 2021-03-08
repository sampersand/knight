namespace Knight.Ops
{
	public interface IAdd
	{
		IValue Add(IValue rhs);
	}

	public interface ISub
	{
		IValue Sub(IValue rhs);
	}

	public interface IMul
	{
		IValue Mul(IValue rhs);
	}

	public interface IDiv
	{
		IValue Div(IValue rhs);
	}

	public interface IMod
	{
		IValue Mod(IValue rhs);
	}

	public interface IPow
	{
		IValue Pow(IValue rhs);
	}
}
