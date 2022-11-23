# Taichi ClothSimulation in UE  

### Demo效果

编译运行项目后，在编辑器界面内容浏览器选择MyAssets/Level/TaichiCLSI.umap即可查看布料模拟效果，效果如下所示。

![2](./Taichi_clsi_Demo.gif)

### 实现思路

具体实现思路与FreeFall文件类似

注意利用TaichiAOT模式下对应C_API的接口传递参数，参考官方AOT_DEMO。

将位置赋值给网格体Pos。

注意Asset中的Taichi_Free_AOT文件存放Taichi通过python生成的taichi运行时需要的kernel文件，不要使用和删除该文件夹。

### 参考

https://github.com/taichi-dev/taichi-aot-demo/blob/master/tutorial.md

https://github.com/taichi-dev/taichi-aot-demo/tree/master/0_tutorial_cgraph
