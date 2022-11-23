# ComputeShader FreeFall  

### Demo效果

编译运行项目后，在编辑器界面内容浏览器选择MyAssets/level/CSFree.CSFree即可查看自由自由落体效果，效果如下所示。

![2](./CS_FreeFall_Demo.gif)

### 实现思路

使用computeshader计算获取小球位置。

将位置传递给球体静态网格体。

### 参考

https://docs.unrealengine.com/5.0/zh-CN/creating-a-new-global-shader-as-a-plugin-in-unreal-engine/

https://forums.unrealengine.com/t/tutorial-pixel-and-compute-shaders-in-ue4/20593

https://github.com/aceyan/UE4_GPGPU_flocking/tree/ComputeShader

https://github.com/XiaoyuXiao1998/UE4FreeFallGPU
