NeoWeather: A DDE Dock Plugin
==============================

**This repo is no longer actively maintaining, due to breaking changes from deepin and lack of API docs**


This is a weather plugin for deepin-dock, with API from [openweathermap](https://openweathermap.org).

### Installation Guide ###
For Arch users, there's AUR version [deepin-dock-plugin-neoweather](https://aur.archlinux.org/packages/deepin-dock-plugin-neoweather/).

For other DDE users, you can compile it either using Qt Creator or by the following command: 

```
mkdir build
cd build
qmake ../source
make
```

And than install

```
sudo make install
```

### TODO list ###

- [X] Add i18n support
- [X] Add Night Icons to themes
- [ ] Cityid look up
- [ ] Auto city

### Acknowledgment ###
This Plugin was initially inspired by [sonichy's weather plugin](https://github.com/sonichy/WEATHER_DDE_DOCK). 

Some icons are from [zondicons](http://www.zondicons.com/)

@caglarturali provides Turkish translation. 
