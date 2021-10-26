# Chương trình có thể chơi nhạc trong thư mục Music của hệ điều hành  
## Tổng quan
1. Load tất cả các bài hát có định dạng mp3 ở thư mục Music tương ứng hệ điều hành

- Khi ứng dụng khởi động sẽ tự động load tất cả các file mp3 trong thư mục Music mặc định của hệ điều hành vào ứng dụng project


2. Xây dựng ứng dụng với **Media player** và **Model bằng C++**

- Thay vì sử dụng ListModel của QML thì ứng dụng này sẽ xây dựng model bằng đối tượng QAbstractListModel

3. Giao diện xây dựng bằng QML, song các chức năng hay dữ liệu hiển thị được thực hiện bằng C++.

- Giữ nguyên giao diện với QML, liên kết các thao tác trên giao diện với các logic chơi nhạc, danh sách bài hát được viết bằng C++

4. Sử dụng thư viện bên thứ ba *(taglib)* với ứng dụng Qt 

- Thư viện taglib cung cấp giải pháp cho việc đọc metadata của file mp3, cho phép người dùng trích xuất tiêu đề, ca sĩ cũng như album art của bài hát đó.

## Học được  
1.   Xây đựng ứng dụng theo mô hình Model View, trong đó QML đảm nhận vai trò view còn model là C++.

2.   Thực hành được giao tiếp giữa QML và C++

3.   Sử dụng thư viện bên thứ ba với ứng dụng Qt