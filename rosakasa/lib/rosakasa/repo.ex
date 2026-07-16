defmodule Rosakasa.Repo do
  use Ecto.Repo,
    otp_app: :rosakasa,
    adapter: Ecto.Adapters.Postgres
end
