# rnnoise
RNNoise is a noise suppression library based on a recurrent neural network

## Usage
To compile, just type:
```bash
% ./autogen.sh
% ./configure
% make
```

Optionally:
```bash
% make install
```

### denoise training
Compile first as follows:
```bash
% cd src
% ./compile.sh
```

train denoise.
```bash
% ./denoise_training <speech> <noise> <output denoised>
...
matrix size: N x M // This value is immediately reused as the next argument.
```

### make denoise_data9.h5
Change the previously created binary to the HDF5 binary data format.
```bash
% cd training
% ./bin2hdf5.py output.f32 N M denoise_data9.h5
```

### rnn train
```bash
% cd training
% ./rnn_train.py
```
This will save the model under the name `newweights9i.hdf5`.

### save the hyperparameter
Save the hyperparameter from the `newweights9i.hdf5` file to the C file.
```bash
% cd training
% ./dump_rnn.py newweights9i.hdf5 ../src/rnn_data.c ../src/rnn_data.h
```

### recompile demo
Recompile the demo based on the newly written `rnn_data.c`.
```bash
% make clean && make
```

## demo
While it is meant to be used as a library, a simple command-line tool is
provided as an example. It operates on RAW 16-bit (machine endian) mono
PCM files sampled at 48 kHz. It can be used as:
```
% ./examples/rnnoise_demo input.pcm output.pcm
```

The output is also a 16-bit raw PCM file.
