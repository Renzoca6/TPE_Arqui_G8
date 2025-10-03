#!/usr/bin/env bash
set -euo pipefail

# ==== CONFIGURACIÓN ====
NAME="${1:-tp-arq}"

MI_USER="${MI_USER:-}"

docker_exec() {
  if [[ -n "$MI_USER" ]]; then
    docker exec -u "$MI_USER" -it "$NAME" "$@"
  else
    docker exec -it "$NAME" "$@"
  fi
}

# ==== FLUJO ====
echo "[1/5] Iniciando contenedor: $NAME"
docker start "$NAME" >/dev/null

echo "[2/5] Limpieza Toolchain"
docker_exec make clean -C /root/Toolchain

echo "[3/5] Limpieza raíz del proyecto"
docker_exec make clean -C /root/

echo "[4/5] Compilando Toolchain"
docker_exec make -C /root/Toolchain

echo "[5/5] Compilando proyecto"
docker_exec make -C /root/

# echo "Deteniendo contenedor..."
# docker stop "$NAME" >/dev/null

echo "✅ Compilación completa. Ahora podés ejecutar ./run.sh desde WSL."