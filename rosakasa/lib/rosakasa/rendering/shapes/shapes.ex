defmodule Rosakasa.Rendering.Shapes do
  use Rosakasa.Rendering.Renderer

  alias Rosakasa.Rendering.Renderer

  # Scales the 0..255 wire `area` byte up so a full byte spans a screen-sized
  # circle (radius ~120 = 255 * @area_scale worth of area).
  @area_scale 177

  @spec draw_square(Renderer.point(), pos_integer(), integer()) :: [Renderer.span()]
  def draw_square(center, length, intensity) when length > 0 do
    draw_rectangle(center, length, length, intensity)
  end

  @spec draw_rectangle(Renderer.point(), pos_integer(), pos_integer(), integer()) :: [Renderer.span()]
  def draw_rectangle(center, width, height, intensity) do
    {center_x, center_y} = Renderer.normalize_point(center)
    x = center_x - div(width, 2)
    y = center_y - div(height, 2)

    y..(y + height - 1)
    |> Enum.map(fn row ->
      Renderer.draw_span(row, x, x + width - 1, intensity)
    end)
  end

  @spec draw_circle(Renderer.point(), integer, integer()) :: [Renderer.span()]
  def draw_circle(center, area, intensity) do
    {center_x, center_y} = Renderer.normalize_point(center)

    # The diameter is also the length of the center span
    diameter = 2 * :math.sqrt(area * @area_scale / 3.141)
    radius = diameter / 2
    # Everytime it goes up or down, it loses some percentage of its width

    y = center_y;

    points = y..(round(y + radius))
    |> Enum.map(fn row ->
      offset = get_circle_offset(row, center_y, radius);
      {row, center_x - offset, center_x + offset}
    end)

    points = points ++ (y..(round(y - radius))
    |> Enum.map(fn row ->
      offset = get_circle_offset(row, center_y, radius);
      {row, center_x - offset, center_x + offset}
    end))

      Enum.map(points, fn {row, x, x2} ->
        Renderer.draw_span(row, round(x), round(x2), intensity)
      end)

  end


  defp get_circle_offset(row, y, radius) do
    dy = row - y
    :math.sqrt(max(0.0, (radius**2) - (dy**2)))
  end

end
