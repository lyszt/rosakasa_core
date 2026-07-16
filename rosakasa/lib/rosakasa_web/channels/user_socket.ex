defmodule RosakasaWeb.UserSocket do
  use Phoenix.Socket

  channel "engine:*", RosakasaWeb.EngineChannel

  @impl true
  def connect(_params, socket, _connect_info) do
    # Authorize the ESP32 here later if needed
    {:ok, socket}
  end

  @impl true
  def id(_socket), do: nil
end
