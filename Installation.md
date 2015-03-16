# Installing #

## Packages ##

Links to openSUSE Build Service Packages for (K)Ubuntu, openSUSE, Fedora and Mandriva can be found from the [KDE-Look.org page](http://kde-apps.org/content/show.php?content=20121).

## Source Package ##

Installing source package:
```
tar xjvf kmediafactory-* 
cd kmediafactory-* 
./configure 
make 
sudo make install 
```

## Development Version ##

```
svn checkout http://kmediafactory.googlecode.com/svn/trunk kmediafactory
cd kmediafactory 
mkdir debug 
cd debug 
cmake .. -DCMAKE_BUILD_TYPE=debugfull 
make 
sudo make install
```