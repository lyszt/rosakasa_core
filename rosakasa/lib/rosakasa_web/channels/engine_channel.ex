defmodule RosakasaWeb.EngineChannel do
  use RosakasaWeb, :channel

  alias Rosakasa.Rendering

  @frame_budget_us 1_000

  # 1. The ESP32 connects to this topic
  @impl true
  def join("engine:esp32", _payload, socket) do
    {:ok, socket}
  end

  @impl true
  def handle_in("render_frame", {:binary, data}, socket) do
    started_at = System.monotonic_time(:microsecond)
    commands = Rendering.decode_commands(data)
    spans = Rendering.render_frame_binary(commands)

    if System.monotonic_time(:microsecond) - started_at > @frame_budget_us do
      {:reply, {:ok, {:binary, <<>>}}, socket}
    else
      {:reply, {:ok, {:binary, spans}}, socket}
    end
  end
end
