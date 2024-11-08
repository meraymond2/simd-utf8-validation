// const to12BitString = (n: number): string =>
const to12BitString = (n: number): string => {
  const bits = n.toString(2)
  const padding = "0".repeat(12 - bits.length)
  return padding + bits
}

// Table 8, 2-Byte error patterns
const invalidSequences: [number, RegExp][] = [
  // Too long (ASCII + Continuation)
  // 0|  10|
  [0, /^0.......10..$/],
  // Too Short (Missing 2nd Byte)
  // 11|  0|
  [1, /^11......0...$/],
  // 11|  11|
  [1, /^11......11..$/],
  // Overlong (2-Byte)
  // 110|00000...00001 10|
  [2, /^1100000.10..$/],
  // Surrogate
  // 1110|1101  10|1
  [3, /^11101101101.$/],
  // Overlong (3-Byte)
  // 1110|0000  10|0
  [4, /^11100000100.$/],
  // Overlong (4-Byte)
  [5, /^111100001000$/],
  // + Too Large
  // 1111|0101...1111 10|00
  [5, /^111101011000$/],
  [5, /^111101101000$/],
  [5, /^111101111000$/],
  [5, /^11111...1000$/],
  // Too Large
  // 1111|0100...1111 10|01...11
  [6, /^1111010010(01|10|11)$/],
  [6, /^1111010110(01|10|11)$/],
  [6, /^1111011010(01|10|11)$/],
  [6, /^1111011110(01|10|11)$/],
  [6, /^11111...10(01|10|11)$/],
  [7, /^10......10..$/],
]

const flagsToErrorBit = (flags: number[]): number => {
  const deduped = Array.from(new Set(flags))
  const errorBit = deduped.reduce((acc, flag) => acc | (1 << flag), 0)
  return errorBit
}

const main = () => {
  let i = 0
  let firstHighNibble: Record<string, number[]> = {}
  let firstLowNibble: Record<string, number[]> = {}
  let secondHighNibble: Record<string, number[]> = {}
  while (i < 4096) {
    const bits = to12BitString(i)
    // These are the actual error codes for each possible 12-bit sequence. Each
    // number has either zero or one flag.
    const flags = invalidSequences.reduce<number[]>((acc, [flag, rgx]) => (rgx.test(bits) ? acc.concat(flag) : acc), [])
    // Now we save those flags against each third of the sequence. For each
    // nibble, these are only now possible flags. If a given combination of
    // nibbles all have a flag, then it means that combination has that error
    // code.
    const first = bits.slice(0, 4)
    const second = bits.slice(4, 8)
    const third = bits.slice(8, 12)

    firstHighNibble[first] = firstHighNibble[first] ? firstHighNibble[first].concat(flags) : flags
    firstLowNibble[second] = firstLowNibble[second] ? firstLowNibble[second].concat(flags) : flags
    secondHighNibble[third] = secondHighNibble[third] ? secondHighNibble[third].concat(flags) : flags

    i++
  }
  // For each table, reduce the list of flags down to a number representing a
  // combination of error bits.
  const firstHighReduced = Object.fromEntries(
    Object.entries(firstHighNibble).map(([nibble, flags]) => [nibble, flagsToErrorBit(flags)])
  )
  const firstLowReduced = Object.fromEntries(
    Object.entries(firstLowNibble).map(([nibble, flags]) => [nibble, flagsToErrorBit(flags)])
  )
  const secondHighReduced = Object.fromEntries(
    Object.entries(secondHighNibble).map(([nibble, flags]) => [nibble, flagsToErrorBit(flags)])
  )
  // Finally, convert them to sorted arrays. (I could do this without the intermediate
  // record, but it was easier for debugging.)
  const table1 = Object.entries(firstHighReduced).reduce<number[]>((acc, [nibble, errorBit]) => {
    const idx = parseInt(nibble, 2)
    acc[idx] = errorBit
    return acc
  }, Array(16))
  const table2 = Object.entries(firstLowReduced).reduce<number[]>((acc, [nibble, errorBit]) => {
    const idx = parseInt(nibble, 2)
    acc[idx] = errorBit
    return acc
  }, Array(16))
  const table3 = Object.entries(secondHighReduced).reduce<number[]>((acc, [nibble, errorBit]) => {
    const idx = parseInt(nibble, 2)
    acc[idx] = errorBit
    return acc
  }, Array(16))

  console.log(table1)
  console.log(table2)
  console.log(table3)
}

main()
