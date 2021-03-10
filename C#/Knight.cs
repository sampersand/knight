using System;
using System.IO;
using System.Linq;

namespace Knight
{
	public class Kn
	{

		internal static IValue Parse(Stream stream) {
			while (!stream.IsEmpty()) {
				// strip comments.
				if (stream.TakeWhileIfStartsWith('#', c => c != '\n') != null)
					continue;

				// strip whitespace.
				if (stream.TakeWhile(c => char.IsWhiteSpace(c) || "(){}[]:".Contains(c)) != null)
					continue;

				// if we neither had comments or whitespace, break out.
				break;
			}

			if (stream.IsEmpty())
				return null;

			return Number.Parse(stream) ??
				Boolean.Parse(stream) ?? 
				String.Parse(stream) ??
				Null.Parse(stream) ??
				(IValue) Identifier.Parse(stream) ??
				Function.Parse(stream);
		}

		public static IValue Run(string stream) => Run(new Stream(stream));
		public static IValue Run(Stream stream) {
			if (stream.IsEmpty()) {
				throw new ParseException("nothing to parse.");
			}

			IValue value = Parse(stream);
			
			if (value == null) {
				throw new ParseException($"Unknown token start '{stream.Take()}'.");
			} else {
				return value.Run();
			}
		}

		static int Main(string[] args) {
			if (args.Length != 2 || args[0] != "-e" && args[0] != "-f") {
				Console.Error.WriteLine("usage: {0} (-e 'program' | -f file)", Environment.GetCommandLineArgs()[0]);
				return 1;
			}

			try {
				Run(args[0] == "-e" ? args[1] : File.ReadAllText(args[1]));
				return 0;
			} catch (KnightException err) {
				Console.Error.WriteLine("invalid program: {0}", err.Message);
				return 1;
			}
		}
	}
}