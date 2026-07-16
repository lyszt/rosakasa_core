defmodule Rosakasa.Rendering do
  @moduledoc """
  Pure rendering math for converting shapes into framebuffer pixel commands.

  Coordinates use the same convention as the engine framebuffer: `{0, 0}` is
  the top-left corner, x grows right, and y grows down.
  """

  defmacro __using__(_opts) do
    quote do
      import Rosakasa.Rendering
    end
  end

  @type point :: {integer(), integer()} | %{x: integer(), y: integer()}
  @type pixel :: %{type: String.t(), x: integer(), y: integer(), intensity: 0..255}
  @type span :: %{
          type: String.t(),
          y: integer(),
          x_start: integer(),
          x_end: integer(),
          intensity: 0..255
        }

  @screen_width 240
  @screen_height 240

  @spec draw_pixel(point(), integer()) :: pixel()
  def draw_pixel(point, intensity) do
    {x, y} = normalize_point(point)

    %{
      type: "pixel",
      x: x,
      y: y,
      intensity: clamp_intensity(intensity)
    }
  end

  @spec draw_span(integer(), integer(), integer(), integer()) :: span()
  def draw_span(y, x_start, x_end, intensity) do
    {x_start, x_end} =
      if x_start <= x_end do
        {x_start, x_end}
      else
        {x_end, x_start}
      end

    %{
      type: "span",
      y: y,
      x_start: x_start,
      x_end: x_end,
      intensity: clamp_intensity(intensity)
    }
  end

  @spec draw_line(point(), point(), integer()) :: [pixel() | span()]
  def draw_line(start, finish, intensity) do
    {x0, y0} = normalize_point(start)
    {x1, y1} = normalize_point(finish)

    if y0 == y1 do
      [draw_span(y0, x0, x1, intensity)]
    else
      dx = abs(x1 - x0)
      dy = -abs(y1 - y0)
      sx = if x0 < x1, do: 1, else: -1
      sy = if y0 < y1, do: 1, else: -1

      line_points(x0, y0, x1, y1, dx, dy, sx, sy, dx + dy, intensity, [])
      |> pixels_to_spans()
    end
  end

  @spec draw_square(point(), pos_integer(), integer()) :: [span()]
  def draw_square(center, length, intensity) when length > 0 do
    {center_x, center_y} = normalize_point(center)
    x = center_x - div(length, 2)
    y = center_y - div(length, 2)

    y..(y + length - 1)
    |> Enum.map(fn row ->
      draw_span(row, x, x + length - 1, intensity)
    end)
  end

  @spec render_frame([map()]) :: [pixel() | span()]
  def render_frame(commands) when is_list(commands) do
    Enum.flat_map(commands, &render_command/1)
  end

  @spec render_frame_binary([map()]) :: binary()
  def render_frame_binary(commands) when is_list(commands) do
    commands
    |> render_frame()
    |> Enum.flat_map(&shape_to_spans/1)
    |> Enum.flat_map(&clip_span/1)
    |> Enum.map(&span_to_binary/1)
    |> IO.iodata_to_binary()
  end

  @spec decode_commands(binary()) :: [map()]
  def decode_commands(data) when is_binary(data) do
    decode_commands(data, [])
  end

  defp decode_commands(<<>>, commands), do: Enum.reverse(commands)

  defp decode_commands(<<1, x, y, _c, _d, intensity, rest::binary>>, commands) do
    command = %{
      "type" => "pixel",
      "point" => %{"x" => x, "y" => y},
      "intensity" => intensity
    }

    decode_commands(rest, [command | commands])
  end

  defp decode_commands(<<2, x0, y0, x1, y1, intensity, rest::binary>>, commands) do
    command = %{
      "type" => "line",
      "start" => %{"x" => x0, "y" => y0},
      "end" => %{"x" => x1, "y" => y1},
      "intensity" => intensity
    }

    decode_commands(rest, [command | commands])
  end

  defp decode_commands(<<3, x, y, length, _d, intensity, rest::binary>>, commands) do
    command = %{
      "type" => "square",
      "center" => %{"x" => x, "y" => y},
      "length" => length,
      "intensity" => intensity
    }

    decode_commands(rest, [command | commands])
  end

  defp decode_commands(_invalid, commands), do: Enum.reverse(commands)

  defp render_command(%{"type" => "pixel", "point" => point} = command) do
    [draw_pixel(point, Map.get(command, "intensity", 255))]
  end

  defp render_command(%{"type" => "line", "start" => start, "end" => finish} = command) do
    draw_line(start, finish, Map.get(command, "intensity", 255))
  end

  defp render_command(%{"type" => "square", "center" => center, "length" => length} = command) do
    draw_square(center, length, Map.get(command, "intensity", 255))
  end

  defp render_command(_command), do: []

  defp shape_to_spans(%{type: "span"} = span), do: [span]
  defp shape_to_spans(%{type: "pixel"} = pixel), do: pixels_to_spans([pixel])

  defp span_to_binary(%{y: y, x_start: x_start, x_end: x_end, intensity: intensity}) do
    <<clamp_byte(y), clamp_byte(x_start), clamp_byte(x_end), clamp_byte(intensity)>>
  end

  defp clip_span(%{y: y}) when y < 0 or y >= @screen_height, do: []

  defp clip_span(%{x_start: x_start, x_end: x_end}) when x_end < 0 or x_start >= @screen_width do
    []
  end

  defp clip_span(span) do
    [
      %{
        span
        | x_start: max(span.x_start, 0),
          x_end: min(span.x_end, @screen_width - 1)
      }
    ]
  end

  defp pixels_to_spans([]), do: []

  defp pixels_to_spans(pixels) do
    pixels
    |> Enum.group_by(fn %{y: y, intensity: intensity} -> {y, intensity} end, & &1.x)
    |> Enum.flat_map(fn {{y, intensity}, xs} ->
      xs
      |> Enum.uniq()
      |> Enum.sort()
      |> xs_to_spans(y, intensity)
    end)
    |> Enum.sort_by(fn %{y: y, x_start: x_start} -> {y, x_start} end)
  end

  defp xs_to_spans([], _y, _intensity), do: []

  defp xs_to_spans([x | xs], y, intensity) do
    {spans, start, finish} =
      Enum.reduce(xs, {[], x, x}, fn next_x, {spans, start, finish} ->
        if next_x == finish + 1 do
          {spans, start, next_x}
        else
          {[draw_span(y, start, finish, intensity) | spans], next_x, next_x}
        end
      end)

    Enum.reverse([draw_span(y, start, finish, intensity) | spans])
  end

  defp line_points(x0, y0, x1, y1, dx, dy, sx, sy, error, intensity, pixels) do
    pixels = [draw_pixel({x0, y0}, intensity) | pixels]

    if x0 == x1 and y0 == y1 do
      Enum.reverse(pixels)
    else
      e2 = 2 * error

      {x0, error} =
        if e2 >= dy do
          {x0 + sx, error + dy}
        else
          {x0, error}
        end

      {y0, error} =
        if e2 <= dx do
          {y0 + sy, error + dx}
        else
          {y0, error}
        end

      line_points(x0, y0, x1, y1, dx, dy, sx, sy, error, intensity, pixels)
    end
  end

  defp normalize_point({x, y}) when is_integer(x) and is_integer(y), do: {x, y}
  defp normalize_point(%{x: x, y: y}) when is_integer(x) and is_integer(y), do: {x, y}
  defp normalize_point(%{"x" => x, "y" => y}) when is_integer(x) and is_integer(y), do: {x, y}

  defp clamp_intensity(intensity) when intensity < 0, do: 0
  defp clamp_intensity(intensity) when intensity > 255, do: 255
  defp clamp_intensity(intensity), do: intensity

  defp clamp_byte(value) when value < 0, do: 0
  defp clamp_byte(value) when value > 255, do: 255
  defp clamp_byte(value), do: value
end
