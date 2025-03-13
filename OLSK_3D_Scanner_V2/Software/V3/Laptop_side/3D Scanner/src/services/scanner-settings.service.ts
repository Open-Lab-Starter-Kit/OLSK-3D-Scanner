// Function to extract color values from an RGBA string
export const getColorValues = (rgbaString: string) => {
  const rgbaPattern =
    /^rgba?\((\d+),\s*(\d+),\s*(\d+),\s*([0-9]*\.?[0-9]+)?\)$/;
  const matches = rgbaString.match(rgbaPattern);

  if (matches) {
    const red = parseInt(matches[1]);
    const green = parseInt(matches[2]);
    const blue = parseInt(matches[3]);
    const intensity = Math.round(parseFloat(matches[4]) * 100);

    return { red, green, blue, intensity };
  } else {
    throw new Error('Invalid RGBA format');
  }
};
