use std::io::{self, Write};

fn main() {
    // 행의 크기 입력
    print!("행의 크기를 입력하세요: ");
    io::stdout().flush().unwrap();

    let mut input = String::new();
    io::stdin().read_line(&mut input).unwrap();
    let rows: usize = input.trim().parse().unwrap();

    // 열의 크기 입력
    print!("열의 크기를 입력하세요: ");
    io::stdout().flush().unwrap();

    input.clear();
    io::stdin().read_line(&mut input).unwrap();
    let cols: usize = input.trim().parse().unwrap();

    // 입력받은 크기에 맞게 행렬 동적 생성
    let mut matrix_a = vec![vec![0; cols]; rows];
    let mut matrix_b = vec![vec![0; cols]; rows];
    let mut result = vec![vec![0; cols]; rows];

    // 첫 번째 행렬 입력
    println!("\n첫 번째 행렬의 원소를 입력하세요.");

    for i in 0..rows {
        for j in 0..cols {
            print!("A[{}][{}] = ", i, j);
            io::stdout().flush().unwrap();

            input.clear();
            io::stdin().read_line(&mut input).unwrap();

            matrix_a[i][j] = input.trim().parse().unwrap();
        }
    }

    // 두 번째 행렬 입력
    println!("\n두 번째 행렬의 원소를 입력하세요.");

    for i in 0..rows {
        for j in 0..cols {
            print!("B[{}][{}] = ", i, j);
            io::stdout().flush().unwrap();

            input.clear();
            io::stdin().read_line(&mut input).unwrap();

            matrix_b[i][j] = input.trim().parse().unwrap();
        }
    }

    // 두 행렬 덧셈
    for i in 0..rows {
        for j in 0..cols {
            result[i][j] = matrix_a[i][j] + matrix_b[i][j];
        }
    }

    // 행렬 A 출력
    println!("\n행렬 A");
    for i in 0..rows {
        for j in 0..cols {
            print!("{} ", matrix_a[i][j]);
        }
        println!();
    }

    // 행렬 B 출력
    println!("\n행렬 B");
    for i in 0..rows {
        for j in 0..cols {
            print!("{} ", matrix_b[i][j]);
        }
        println!();
    }

    // 결과 행렬 출력
    println!("\n두 행렬의 합");
    for i in 0..rows {
        for j in 0..cols {
            print!("{} ", result[i][j]);
        }
        println!();
    }
}
