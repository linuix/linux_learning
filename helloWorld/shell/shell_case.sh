# !bin/bash
echo today is sunday ?
read timeOfDay

case $timeOfDay in
yes ) echo "i\`m every happy !" ;;
no  ) echo "i\`m sead !" ;;
* ) echo "input error !";;
esac

echo today is sun ?
read todayOfTime
case "$todayOfTime" in
yes | y) echo "i'm happy !" ;;
no | n | N) echo "i'm sead !" ;;
*) echo "input error !" ;;
esac

exit 0
