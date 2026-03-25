# create project
drogon_ctl create project drogon_crud_demo

# build project
make build

# run project
make start

# test project 
cd build && ctest -V

# benchmark api
wrk -t12 -c400 -d30s http://localhost:5555/hello

# sqlite
sqlite3 tasks.db "CREATE TABLE tasks (id INTEGER PRIMARY KEY AUTOINCREMENT, title TEXT, todo_date TEXT);"

# create models
drogon_ctl create model ./modules/hospital/models