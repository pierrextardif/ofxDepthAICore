# ofxDepthAICore
[DepthAI-Core](https://github.com/luxonis/depthai-core) addon for [Openframeworks](https://openframeworks.cc/)

<br>
<br>


Results for NeuralNetwork Example (Semantic Segmentation with depth ):

![.](./Assets/imgs/results.gif)

<br>

# Getting Started

<br>

1) Install Openframeworks.

<br>

2) Install DepthAI-Core.

<br>

3) clone this repository inside your `OF_PATH/addons`

<br>

4) add `ofxCMake` to your addons.

<br>

5) Add the [ofxDepthAICore.cmake](./ofxDepthAICore.cmake) to the external folder of ofxCMake which is [here](https://github.com/saynono/ofxCMake/tree/master/addOns/external).

<br>

Alternatively you can change the path of ofxDepthai-Core to your own, by changing this line.


<br>

6) Run the `cmake ..` command from one of the examples .For example, for the [NeuralNetworkExample](./NeuralNetworkExample) :

```sh
cd NeuralNetworkExample
mkdir build
cmake ..
```

You should get an ouutput close to [this one](./Assets/outputLogs/cmakeNN.log)

7) Run the example.

<br>

It is **important** to note that DepthAI-Core in Openframeworks requires to use `sudo`.

<br>

```sh
cd NeuralNetworkExample
cd build
make -j$(n_proc)
cd ../ && cd bin
sudo ./depthAICoreNNExample
```

# Dependencies

[ofxCMake](https://github.com/saynono/ofxCMake).

