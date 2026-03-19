set ws        [lindex $argv 0]
set project   [lindex $argv 1]

setws $ws

app build -name $project

