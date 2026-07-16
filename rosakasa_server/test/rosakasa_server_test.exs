defmodule RosakasaServerTest do
  use ExUnit.Case
  doctest RosakasaServer

  test "greets the world" do
    assert RosakasaServer.hello() == :world
  end
end
