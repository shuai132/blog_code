# Docker最佳实践

所谓最佳实践，也就是个人总结的一些习惯和约定，以更好地使用Docker。
不规范/过于随意的使用，会生成大量的中间层镜像和临时容器，
这带来大量的难以识别的镜像和容器名字，不利于镜像/容器管理。

Docker的安装和使用可参考这个手册：[https://github.com/yeasy/docker_practice](https://github.com/yeasy/docker_practice)

## 实践

首要一个准则就是: **使用docker-compose而不是一长串的docker命令/shell脚本去配置和运行容器。**
接下来就是说明**如何规范地使用docker-compose**：

推荐使用portainer(一个Docker图形化管理工具)管理和查看docker。下面也以这个镜像的使用为例，说明最佳的使用方式，具体操作如下：

* 新建portainer文件夹并进入
* 新建docker-compose.yml文件:

```yaml
version: "3"
services:
  app:
    container_name: portainer
    image: docker.io/portainer/portainer
    volumes:
      - /var/run/docker.sock:/var/run/docker.sock
      - ./data:/data
    ports:
      - "9000:9000"
    restart: unless-stopped
```

* 运行

```bash
docker-compose up -d
```

就会自动下载镜像并运行容器，然后我们可以访问[http://localhost:9000](http://localhost:9000)来进入管理界面。

## 分析

以上操作有几个重要的点：

* docker-compose.yml所在目录
这个目录，要放在自己项目的根目录（主要是为了目录名是项目名），因为这个目录名将作为后续镜像和容器名称的前缀。

* container_name
大多数情况，我们不需要容器是可扩展的，为了更明确的管理，推荐始终为container_name属性命名。

就上述的结果而言：

* 镜像名为: portainer/portainer:latest
* 容器名为: portainer
* 网络名为: portainer_default
* 属于一个Stack: portainer

可见，docker-compose所在文件夹对于自动生成的各种名字至关重要。假如把它放在项目的docker文件夹，尽管看上去更规范，实际上当多个项目采取同样的策略时将会有很多问题。且无法很容易分辨镜像和容器属于哪个项目。

注: 尽管我们可以通过docker-compose的一个命令行参数指定这个名字，依然不推荐这么用。原因是，要么你要写到shell里，多了一层，要么需要每次手动输入，毫无约束。

## Links

* 文章示例代码: [https://github.com/shuai132/blog_code/tree/master/doc/4](https://github.com/shuai132/blog_code/tree/master/doc/4)
