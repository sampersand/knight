<?php

namespace Knight;

class Stream {
	private $stream;

	public function __construct(string $stream) {
		$this->stream = $stream;
	}

	public function strip(): void {
		$this->stream = preg_replace('/\A(?:[\]\[\s(){}:]+|\#[^\n]*\n)*/m', '', $this->stream);
	}

	public function match(string $regex, int $idx=0): ?string {
		if (!preg_match("/\A(?:$regex)/m", $this->stream, $match)) {
			return null;
		}

		$this->stream = substr($this->stream, strlen($match[0]));

		return $match[$idx];
	}
}
