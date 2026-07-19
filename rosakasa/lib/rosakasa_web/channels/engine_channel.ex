defmodule RosakasaWeb.EngineChannel do
  use RosakasaWeb, :channel

  alias Rosakasa.Rendering.Renderer

  # A full-frame CLEAR expands to ~240 background spans on top of the shapes, so
  # the per-frame rasterization budget is generous enough not to drop those frames.
  @frame_budget_us 5_000

  # 1. The ESP32 connects to this topic
  @impl true
  def join("engine:esp32", _payload, socket) do
    {:ok, socket}
  end

  @impl true
  def handle_in("render_frame", {:binary, data}, socket) do
    started_at = System.monotonic_time(:microsecond)
    commands = Renderer.decode_commands(data)
    spans = Renderer.render_frame_binary(commands)

    if System.monotonic_time(:microsecond) - started_at > @frame_budget_us do
      {:reply, {:ok, {:binary, <<>>}}, socket}
    else
      {:reply, {:ok, {:binary, spans}}, socket}
    end
  end
end
