/**
 * The parent class for all errors that can arise within Knight.
 */
export class KnightError extends Error { }

/**
 * The class that represents errors that can occur during the parsing of a
 * Knight program.
 */
export class ParseError extends KnightError { }

/**
 * The class that represents errors that can occur during the execution of a
 * Knight program.
 */
export class RuntimeError extends KnightError { }
