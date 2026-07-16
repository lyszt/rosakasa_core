defmodule RosakasaWeb.PageController do
  use RosakasaWeb, :controller

  def home(conn, _params) do
    render(conn, :home)
  end
end
