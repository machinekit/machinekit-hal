__machinekit-hal__ is a split out repo which just contains the HAL elements of machinekit

It can be built as a RIP and used in the same way as a machinekit RIP, but without the CNC elements.

Install _machinekit-hal-{flavour}_  if using packages.

PR's should not be made directly to this repo without prior notice.

The machinekit repo is periodically cherry-picked for relevant new commits by the developers
and machinekit-hal updated from these.

NB. There is a related repo __machinekit-cnc__, which contains all the CNC elements missing from this repo.

Now create a PR against the machinekit repo.

## Python

### Installation via PyPi
To use the Machinetalk protobuf Python modules in your projects, use:

```sh
pip install machinetalk-protobuf
```

### Installation from source
Alternatively you can install the Python modules directly from the source code.

```sh
make
python setup.py build
sudo python setup.py install
```

### Usage
See [examples](python/examples). 

## JavaScript (NPM/NodeJS)

### Installation

To use machinetalk protobuf definitions in your npm-based projects, use:

```sh
npm install --save machinetalk-protobuf
```

### Usage

See [examples](js/examples). If you want to try these examples, be sure to first run `npm install` in this repository.

#### Encoding

```js
var machinetalkProtobuf = require('machinetalk-protobuf');
var messageContainer = {
  type: machinetalkProtobuf.message.ContainerType.MT_PING
};
var encodedMessageContainer = machinetalkProtobuf.message.Container.encode(messageContainer);
```
This results in a buffer that starts with `0x08 0xd2 0x01`.

#### Decoding

```js
var machinetalkProtobuf = require('machinetalk-protobuf');
var encodedBuffer = new Buffer([0x08, 0xd2, 0x01]);
var decodedMessageContainer = machinetalkProtobuf.message.Container.decode(encodedBuffer);
```
This results in a messageContainer like the one defined in [Encoding](#Encoding).

## JavaScript (Browser)

### CDN usage
```html
<script src="//cdn.rawgit.com/machinekit/machinetalk-protobuf/VERSION/dist/machinetalk-protobuf.js"></script>
```
With `VERSION` replaced by [a valid tag](https://github.com/machinekit/machinetalk-protobuf/releases) or just `master` for testing
the latest master build.

### Encoding

```js
var messageContainer = {
  type: machinetalk.protobuf.message.ContainerType.MT_PING
};
var encodedMessageContainer = machinetalk.protobuf.message.Container.encode(messageContainer);
```
This results in a buffer that starts with `0x08 0xd2 0x01`.

#### Decoding

```js
var encodedBuffer = new ArrayBuffer([0x08, 0xd2, 0x01]);
var decodedMessageContainer = machinetalk.protobuf.message.Container.decode(encodedBuffer);
```
This results in a messageContainer like the one defined in [Encoding](#Encoding).
