using System.Collections.Generic;
using System;
using Knight.Ops;
using System.Diagnostics;

namespace Knight
{
	public class Function : NonIdempotent
	{
		private static  Dictionary<char, (FunctionBody, int)> FUNCTIONS = new Dictionary<char, (FunctionBody, int)>();
		public delegate IValue FunctionBody(params IValue[] args);

		private FunctionBody _function;
		private IValue[] _args;
		private char _name;

		private Function(FunctionBody function, char name, IValue[] args) {
			_function = function;
			_args = args;
			_name = name;
		}

		public override IValue Run() => _function(_args);
		public override void Dump() {
			Console.Write("Function(");

			foreach (var arg in _args) {
				Console.Write($", {arg}");
			}

			Console.Write(")");
		}

		public static void Register(char name, int arity, FunctionBody body) => FUNCTIONS[name] = (body, arity);

		public static Function Parse(Stream stream) {
			(FunctionBody, int) func = (null, 0);
			char name;

			if (!stream.StartsWith(c => FUNCTIONS.TryGetValue(c, out func)))
				return null;

			if (char.IsUpper(name = stream.Take()))
				stream.StripKeyword();

			var args = new IValue[func.Item2];

			for (int i = 0; i < func.Item2; ++i) {
				if ((args[i] = Kn.Parse(stream)) == null) {
					throw new ParseException($"Unable to parse variable '{i}' for function '{name}'.");
				}
			}

			return new Function(func.Item1, name, args);
		}

		private static readonly Random RANDOM = new Random();

		static Function() {
#if !EMBEDDED
			Register('P', 0, args => new String(Console.ReadLine()));
#endif

			Register('R', 0, args => new Number(RANDOM.Next()));
			Register('E', 1, args => Knight.Kn.Run(args[0].ToString()));
			Register('B', 1, args => args[0]);
			Register('C', 1, args => args[0].Run().Run());

#if !EMBEDDED
			Register('`', 1, args => {
				Process proc = new Process();
				// Redirect the output stream of the child process.
				proc.StartInfo.UseShellExecute = false;
				proc.StartInfo.RedirectStandardOutput = true;
				proc.StartInfo.FileName = "/bin/sh";
				proc.StartInfo.ArgumentList.Add("-c");
				proc.StartInfo.ArgumentList.Add(args[0].ToString());
				proc.Start();
				string output = proc.StandardOutput.ReadToEnd();
				proc.WaitForExit();
				return new String(output);
			});
			Register('Q', 1, args => { Environment.Exit((int) args[0].ToNumber()); return null; });
#endif

			Register('!', 1, args => new Boolean(!args[0].ToBoolean()));
			Register('L', 1, args => new Number(args[0].ToString().Length));

#if !EMBEDDED
			Register('D', 1, args => {
				var val = args[0].Run();
				val.Dump();
				return val;
			 });
			Register('O', 1, args => {
				var val = args[0].ToString();

				if (val[val.Length - 1] == '\\') {
					Console.Write(val.Remove(val.Length - 1));
				} else {
					Console.WriteLine(val);
				}

				return new Null();
			});
#endif

			Register('+', 2, args => ((IAdd) args[0].Run()).Add(args[1].Run()));
			Register('-', 2, args => ((ISub) args[0].Run()).Sub(args[1].Run()));
			Register('*', 2, args => ((IMul) args[0].Run()).Mul(args[1].Run()));
			Register('/', 2, args => ((IDiv) args[0].Run()).Div(args[1].Run()));
			Register('%', 2, args => ((IMod) args[0].Run()).Mod(args[1].Run()));
			Register('^', 2, args => ((IPow) args[0].Run()).Pow(args[1].Run()));
			Register('?', 2, args => new Boolean(((IEquatable<IValue>) args[0].Run()).Equals(args[1].Run())));
			Register('<', 2, args => new Boolean(((IComparable<IValue>) args[0].Run()).CompareTo(args[1].Run()) < 0));
			Register('>', 2, args => new Boolean(((IComparable<IValue>) args[0].Run()).CompareTo(args[1].Run()) > 0));
			Register('&', 2, args => {
				var lhs = args[0].Run();
				
				return lhs.ToBoolean() ? args[1].Run() : lhs;
			});
			Register('|', 2, args => {
				var lhs = args[0].Run();
				
				return lhs.ToBoolean() ? lhs : args[1].Run();
			});
			Register(';', 2, args => {
				args[0].Run();
				return args[1].Run();
			});
			Register('=', 2, args => {
				var rhs = args[1].Run();

				((Identifier) args[0]).Assign(rhs);

				return rhs;
			});
			Register('W', 2, args => {
				while (args[0].ToBoolean()) {
					args[1].Run();
				}
				return new Null();
			});

			Register('I', 3, args => args[0].ToBoolean() ? args[1].Run() : args[2].Run());
			Register('G', 3, args => {
				var str = args[0].ToString();
				var start = (int) args[1].ToNumber();
				var length = (int) args[2].ToNumber();

				return new String(str.Length <= start ? "" : str.Substring(start, length));
			});

			Register('S', 4, args => {
				var str = args[0].ToString();
				var start = (int) args[1].ToNumber();
				var length = (int) args[2].ToNumber();
				var repl = args[3].ToString();

				return new String(str.Substring(0, start) + repl + str.Substring(start + length));
			});
			
		}
	}
}