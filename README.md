NeoWeather: A DDE Dock Plugin
==============================

This is a weather plugin for deepin-dock, with API from [openweathermap](https://openweathermap.org).

### Installation Guide ###
For now there's no binary release provided. You can compile it either using Qt Creator or by the following command: 

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
- [ ] Icons with night
- [ ] Cityid look up
- [ ] Auto city

### Acknowledgment ###
This Plugin was initially inspired by [sonichy's weather plugin](https://github.com/sonichy/WEATHER_DDE_DOCK). 

Some icons are from [zondicons](http://www.zondicons.com/)
