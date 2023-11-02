import './Nav.css'
const Nav = () => {
    return (
        <>
          <nav>
            <div className='link'><a href='/'>Home</a></div>
            <div className='link'><a href='/games'>Games</a></div>
            <div className='link'><a href='/hardware'>Hardware</a></div>
            <div className='link'><a href='/software'>Software</a></div>
          </nav>
        </>
    );
};

export default Nav;