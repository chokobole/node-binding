#!/usr/bin/env bash

if ! [ -x "$(command -v nvm)" ]; then
  curl -o- https://raw.githubusercontent.com/creationix/nvm/v0.34.0/install.sh | bash

  source $HOME/.nvm/nvm.sh
fi

NODE_VERSION=v12.14.0

nvm install ${NODE_VERSION}
