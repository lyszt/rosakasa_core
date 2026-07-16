defmodule RosakasaWeb.EngineChannel do
  use RosakasaWeb, :channel

  alias Rosakasa.Rendering

  # 1. The ESP32 connects to this topic
  @impl true
  def join("engine:esp32", _payload, socket) do
    {:ok, socket}
  end

  @impl true
  def handle_in("render_frame", {:binary, data}, socket) do
    commands = Rendering.decode_commands(data)

    {:reply, {:ok, {:binary, Rendering.render_frame_binary(commands)}}, socket}
  end
end
