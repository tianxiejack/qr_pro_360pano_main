cd /home/ubuntu

chmod 777 upgrade_fw

echo "ubuntu" | sudo -S mount /dev/sda1 /home/ubuntu/calib/

echo "ubuntu" | nohup sudo -S ./upgrade_fw > /dev/null 2>&1 &

cd /home/ubuntu/Release
sudo chmod 777 360pano
echo "ubuntu" | nohup sudo -S ./360pano > /dev/null 2>&1 &
