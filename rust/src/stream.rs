#[derive(Debug, Clone)]
pub struct Stream<T> {
	iter: T,
	line: usize,
	peeked: Option<char>
}

impl<'a> Stream<std::str::Chars<'a>> {
	pub fn from_str(stream: &'a str) -> Self {
		Self::new(stream.chars())
	}
}

struct StreamTakeWhile<'a, T, F>(&'a mut Stream<T>, F);

impl<T: Iterator<Item=char>> Stream<T> {
	#[inline]
	pub fn new(iter: T) -> Self {
		Self { iter, line: 0, peeked: None }
	}

	pub fn take_while<'a>(&'a mut self, func: impl Fn(char) -> bool + 'a) -> impl Iterator<Item=char> + 'a {
		StreamTakeWhile(self, func)
	}
}

impl<'a, T, F> Iterator for StreamTakeWhile<'a, T, F>
where
	T: Iterator<Item=char>,
	F: Fn(char) -> bool
{
	type Item = char;

	fn next(&mut self) -> Option<Self::Item> {
		if let Some(peeked) = self.0.peeked.take() {
			if (self.1)(peeked) {
				return Some(peeked);
			} else {
				self.0.peeked.push(peeked);
				return None;
			}
		}

		match self.iter.next() {

		}
	}
}

